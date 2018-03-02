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
			
			std::cout << "Create dir in /tmp: FAILED!";
			std::exit(1);
			
		}
		
		if ((statusCode = system("mkdir /tmp/duppy/known_ap")) != 0 ){
			
			std::cout << "Create dir in /tmp/duppy: FAILED!" << std::endl;
			std::exit(1);
			
		}
}

void removeDirectories(){
		
		int statusCode;
		
		if ((statusCode = system("rm -rf /tmp/duppy")) != 0 ) {
			
			std::cout << "Remove the /tmp/duppy: FAILED! " << std::endl;
			
		}
}

void stopNetworkManager(){
	
		int statusCode;
		
		if ((statusCode = std::system("service network-manager stop")) == 0){
			
			std::cout << "Stop network-manager: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Stop network-manager: FAILED! " << std::endl;
			std::exit(1);
			
		}
}

void startNetworkManager(){
	
		int statusCode;
		
		if ((statusCode = std::system("service network-manager start")) == 0){
			
			std::cout << "Start network-manager: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Start network-manager: FAILED! " << std::endl;
			
		}
}

void ifconfigDown(const char *interfaceName){
	
		int statusCode;
		std::string command = "ifconfig ";
		command.append(interfaceName);
		command.append(" down");
		
		if ((statusCode = std::system(command.c_str())) == 0){
			
			std::cout << "Interface " << interfaceName << " set down: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Interface " << interfaceName << " set down: FAILED !" << std::endl;
			std::exit(1);
			
		}
}

void ifconfigUp(const char *interfaceName){
	
		int statusCode;
		
		std::string command = "ifconfig ";
		command.append(interfaceName);
		command.append(" up");
		
		
		if ((statusCode = std::system(command.c_str())) == 0){
			
			std::cout << "Interface " << interfaceName << " set up: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Interface " << interfaceName << " set up: FAILED! " << std::endl;
			
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
			
			std::cout << "Open /proc/net/dev to check interface: FAILED! " << std::endl;
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
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Hostname change: SUCCESS! ";
			std::cout << "(New hostname: " << hostname << ")" << std::endl;
			
		} else {
			
			std::cout << "Hostname change: FAILED!" << std::endl;
			
		}
}

void saveDefaultHostname(){
	
		int statusCode;
		std::string defaultHostname;
		std::string command = "hostname > /tmp/duppy/hostname";
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Save default hostname: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Save default hostname: FAILED! " << std::endl;
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
		
			std::cout << "Open /tmp/duppy/hostname: FAILED! " << std::endl;;

		}
		
		command += defaultHostname;
		
		if ((statusNumber = system(command.c_str())) == 0){
			
			std::cout << "Hostname set to default: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Hostname set to default: FAILED! " << std::endl;
			
		}
}

void saveDefaultMac(const char *interface){
	
		int statusCode;
		std::string command = "cat /sys/class/net/";
		command.append(interface);
		command.append("/address > /tmp/duppy/defaultmac");
		
		if ((statusCode = system(command.c_str())) == 0 ){
			
			std::cout << "Save default mac address: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Save default mac address: FAILED! " << std::endl;
			
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
		
			std::cout << "Open /tmp/duppy/defaultmac: FAILED!"; 

		}
		
		command += defaultMac;
		
		if ((statusNumber = system(command.c_str())) == 0){
			
			std::cout << "Mac address set to default: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Mac address set to default: FAILED! " << std::endl;
			
		}	
}

void setDuppyNetwork(){
	
		int statusCode;
		std::string command = "cp duppynetwork.conf /etc/NetworkManager/conf.d/";
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Insert duppynetwork.conf: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Insert duppynetwork.conf: FAILED! " << std::endl;
			
		}
}

void deleteDuppynetwork(){
	
		int statusCode;
		std::string command = "rm -f /etc/NetworkManager/conf.d/duppynetwork.conf";
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Remove duppynetwork.conf: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Remove duppynetwork.conf: FAILED! " << std::endl;
			
		}
}

void ignoreArp(const char *interface){
	
		int statusCode;
		int statusCodeTwo;
		
		std::string commandIgnore = "sysctl -q -w net.ipv4.conf.";
		commandIgnore.append(interface);
		commandIgnore.append(".arp_ignore=8");
		
		if ((statusCode = system(commandIgnore.c_str())) == 0){
			
			std::cout << "Ignoring ARP: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Ignoring ARP: FAILED! " << std::endl;
			
		}
		
		sleep(1);
		
		std::string commandAnnounce = "sysctl -q -w net.ipv4.conf.";
		commandAnnounce.append(interface);
		commandAnnounce.append(".arp_announce=2");
		
		if ((statusCodeTwo = system(commandAnnounce.c_str())) == 0){
			
			std::cout << "Restrict ARP announce: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Restrict ARP announce: FAILED! " << std::endl;
			
		}
}




void allowArp(const char *interface){
	
		int statusCode;
		int statusCodeTwo;
		
		std::string commandIgnore = "sysctl -q -w net.ipv4.conf.";
		commandIgnore.append(interface);
		commandIgnore.append(".arp_ignore=0");
		
		if ((statusCode = system(commandIgnore.c_str())) == 0){
			
			std::cout << "Enable ARP: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Enable ARP: FAILED! " << std::endl;
			std::exit(1);
			
		}
		
		std::string commandAnnounce = "sysctl -q -w net.ipv4.conf.";
		commandAnnounce.append(interface);
		commandAnnounce.append(".arp_announce=0");
		
		if ((statusCodeTwo = system(commandAnnounce.c_str())) == 0){
			
			std::cout << "Enable ARP announce: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Enable ARP announce: FAILED! " << std::endl;
			std::exit(1);
			
		}
}

void disableICMP(){
	
		int statusCode;
		std::string command = "sysctl -q -w net.ipv4.icmp_echo_ignore_all=1";
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Disable ping response: SUCCESS!";
			std::cout << " (WARNING!!!! This affect every interface!)" << std::endl;
			
		} else {
			
			std::cout << "Disable ping response: FAILED! " << std::endl;
			
		}
}

void enableICMP(){
	
		int statusCode;
		std::string command = "sysctl -q -w net.ipv4.icmp_echo_ignore_all=0";
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Enable ping response: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Enable ping response: FAILED! " << std::endl;
			
		}
}

void removeKnownAP(){
	
		int statusCode;
		std::string command = "mv -f /etc/NetworkManager/system-connections/* /tmp/duppy/known_ap/";
		
		if ((statusCode = system(command.c_str())) == 0){
			
			std::cout << "Temporary remove known APs: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Temporary remove known APs: FAILED! " << std::endl;
			
		}
}

void setBackKnownAP(){
	
		int statusCode;
		std::string commandMove = "mv -f /tmp/duppy/known_ap/* /etc/NetworkManager/system-connections/";
		std::string commandRm = "rm -f /etc/NetworkManager/system-connections/*";
		
		if ((statusCode = system(commandRm.c_str())) == 0){
			
			std::cout << "Remove the used AP: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Remove the used AP: FAILED! " << std::endl;
			
		}
		
		if ((statusCode = system(commandMove.c_str())) == 0){
			
			std::cout << "Set back known APs: SUCCESS!" << std::endl;
			
		} else {
			
			std::cout << "Set back known APs: FAILED! " << std::endl;
			
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
	if ((argc == 1) || (argc == 2)){
		
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
		
		disableICMP();
		sleep(1);
		
		removeKnownAP();
		sleep(1);
		
		ifconfigUp(selectedInterface.c_str());
		sleep(1);
		
		startNetworkManager();
		sleep(1);
	
		std::cout << "\nDont worry about the \"sudo: unable to resolve host...\" error.\n" << std::endl;
		std::cout << "Duppy has started!\n" << std::endl; 
	
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
		
		std::cout << "\nDuppy has stopped!\n" << std::endl;
	
	} else {
		
		std::cout << "\nUsage: ./duppy stop|start <interface> " << std::endl;
		
	}	
}	 
