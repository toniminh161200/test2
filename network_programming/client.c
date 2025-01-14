#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#define PORT 8888
#define PASS_LENGTH 20

int airplanesys(int sock);
int menu2(int sock, int type);
int do_admin_action(int sock, int action);
int do_action(int sock, int opt);
void view_booking(int sock);
int menu_search(int sock, int search_option);
int main(int argc, char *argv[])
{
	char *ip = "127.0.0.1"; // default ip address if there is no parameter
	if (argc == 2)
	{
		// if there are 2 parameters then the ip will be assigned
		ip = argv[1];
	}

	int cli_fd = socket(AF_INET, SOCK_STREAM, 0); // Socket creation
	//  AF_INET: processes connected by IPv4
	// SOCK_STREAM: Communication type TCP (reliable, connection oriented)
	// Protocol value for Internet Protocol(IP), which is 0
	if (cli_fd == -1)
	{
		printf("socket creation failed\n");
		exit(0);
	}
	struct sockaddr_in ca;				// used to represent an Internet socket address, such as an IP address and a port number.
	ca.sin_family = AF_INET;			// Address family (e.g., AF_INET for IPv4)
	ca.sin_port = htons(PORT);			// This field represents the port number | default: 5000
	ca.sin_addr.s_addr = inet_addr(ip); // holds the IP address in network byte order (big-endian).
	if (connect(cli_fd, (struct sockaddr *)&ca, sizeof(ca)) == -1)
	{
		// used in network programming to establish a connection to a remote server
		// #Socket: A socket is a communication endpoint in a network
		// return 0: connect success | return -1: connection attempt fail
		printf("connect failed\n");
		exit(0);
	}
	printf("connection established\n");

	while (airplanesys(cli_fd) != 3)
		;		   // return 3 <=> exit in airplanesys menu
	close(cli_fd); // close socket
	return 0;
}

int airplanesys(int sock)
{ // param is a socket
	int opt;
	system("clear");
	printf("\t\t******WELCOME TO AIRPLANE BOOKING SYSTEM******\n");
	printf("Choose One Of The Three Option\n");
	printf("1. LogIn\n");
	printf("2. Register\n");
	printf("3. Exit\n");

	scanf("%d", &opt);
	write(sock, &opt, sizeof(opt)); // write to socket option client choose
	if (opt == 1)
	{
		int type, acc_no;
		char password[PASS_LENGTH];
		printf("Enter the Acoount Type:\n");
		printf("1. Customer\n2. Agent\n3. Admin\n");
		printf("Your Response: ");
		scanf("%d", &type);
		printf("Enter Account Number: ");
		scanf("%d", &acc_no);
		strcpy(password, getpass("Enter Password: "));

		write(sock, &type, sizeof(type));
		write(sock, &acc_no, sizeof(acc_no));
		write(sock, &password, strlen(password));

		int valid_login;
		read(sock, &valid_login, sizeof(valid_login));
		if (valid_login == 1)
		{
			while (menu2(sock, type) != -1)
				;
			system("clear");
			return 1;
		}
		else
		{
			printf("Login Not Allowed\n");
			while (getchar() != '\n')
				;
			getchar();
			return 1;
		}
	}
	else if (opt == 2)
	{
		int type, acc_no;
		char password[PASS_LENGTH], secret_pin[5], name[10];
		printf("Enter the type of account:\n");
		printf("1. Customer\n2. Agent\n3. Admin\n");
		printf("Your Response: ");
		scanf("%d", &type);
		printf("Enter Customer Name: ");
		scanf("%s", name);
		strcpy(password, getpass("Enter Password: "));
		if (type == 3)
		{
			int attempt = 1;
			while (1)
			{
				strcpy(secret_pin, getpass("Enter secret PIN to create ADMIN account: "));
				attempt++;
				if (strcmp(secret_pin, "root") != 0 && attempt <= 3)
					printf("Invalid PIN. Please try again.\n");
				else
					break;
			}
			if (!strcmp(secret_pin, "root"))
				;
			else
				exit(0);
		}
		write(sock, &type, sizeof(type));
		write(sock, &name, sizeof(name));
		write(sock, &password, strlen(password));

		read(sock, &acc_no, sizeof(acc_no));
		printf("Remember the account no of further login: %d\n", acc_no);
		while (getchar() != '\n')
			;
		getchar();
		return 2;
	}
	else
		return 3;
}

int menu2(int sock, int type)
{
	int opt = 0;
	if (type == 1 || type == 2)
	{
		system("clear");
		printf("++++ OPTIONS ++++\n");
		printf("1. Book Ticket\n");
		printf("2. View Bookings\n");
		printf("3. Update Booking\n");
		printf("4. Cancel booking\n");
		printf("5. Search flights\n");
		printf("6. Logout\n");
		printf("Your Choice: ");
		scanf("%d", &opt);
		return do_action(sock, opt);
		return -1;
	}
	else
	{
		system("clear");
		printf("++++ OPTIONS ++++\n");
		printf("1. Add Airplane\n");
		printf("2. Delete Airplane\n");
		printf("3. Modify Airplane\n");
		printf("4. Add Root User\n");
		printf("5. Delete User\n");
		printf("6. Logout\n");
		printf("Your Choice: ");
		scanf("%d", &opt);
		return do_admin_action(sock, opt);
	}
}

int do_admin_action(int sock, int opt)
{
	switch (opt)
	{
	case 1:
	{
		int tno;
		char tname[20];
		char departure[50];
		char arrival[50];
		char date[11];
		char boarding_time[7];
		int price;
		write(sock, &opt, sizeof(opt));
		printf("Enter Airplane Name : ");
		scanf(" %[^\n]%*c", tname);
		printf("Airplane Name : %s\n", tname);
		printf("Enter Airplane No. : ");
		scanf("%d", &tno);
		printf("Flight departure : ");
		scanf("%s", departure);
		printf("Flight arrival : ");
		scanf("%s", arrival);
		printf("Price : ");
		scanf("%d", &price);
		printf("Date: ");
		scanf("%s", date);
		printf("Boarding time: ");
		scanf("%s", boarding_time);
		write(sock, &tname, sizeof(tname));
		write(sock, &tno, sizeof(tno));
		write(sock, &departure, sizeof(departure));
		write(sock, &arrival, sizeof(arrival));
		write(sock, &price, sizeof(price));
		write(sock, &date, sizeof(date));
		write(sock, &boarding_time, sizeof(boarding_time));

		read(sock, &opt, sizeof(opt));
		if (opt == 1)
			printf("Airplane Added Successfully.\n");
		while (getchar() != '\n')
			;
		getchar();
		return opt;
		break;
	}
	case 2:
	{
		int no_of_airplanes;
		write(sock, &opt, sizeof(opt));
		read(sock, &no_of_airplanes, sizeof(int));
		// printf("no of airplane:%d",no_of_airplanes);
		while (no_of_airplanes > 0)
		{
			int tid, tno;
			char tname[20];
			read(sock, &tid, sizeof(tid));
			read(sock, &tname, sizeof(tname));
			read(sock, &tno, sizeof(tno));
			if (strcmp(tname, "deleted") != 0)
				printf("%d.\t%d\t%s\n", tid, tno, tname);
			no_of_airplanes--;
		}
		printf("Enter -2 to cancel.\nEnter the airplane ID to delete: ");
		scanf("%d", &no_of_airplanes);
		// printf("no of airplane:%d",no_of_airplanes);
		write(sock, &no_of_airplanes, sizeof(int));
		read(sock, &opt, sizeof(opt));
		if (opt != -2)
			printf("Airplane deleted successfully\n");
		else
			printf("Operation cancelled!");
		while (getchar() != '\n')
			;
		getchar();
		return opt;
		break;
	}
	case 3:
	{
		int no_of_airplanes;
		write(sock, &opt, sizeof(opt));
		read(sock, &no_of_airplanes, sizeof(int));
		while (no_of_airplanes > 0)
		{
			int tid, tno;
			char tname[20];
			read(sock, &tid, sizeof(tid));
			read(sock, &tname, sizeof(tname));
			read(sock, &tno, sizeof(tno));
			if (!strcmp(tname, "deleted"))
				;
			else
				printf("%d.\t%d\t%s\n", tid + 1, tno, tname);
			no_of_airplanes--;
		}
		printf("Enter 0 to cancel.\nEnter the airplane ID to modify: ");
		scanf("%d", &no_of_airplanes);
		write(sock, &no_of_airplanes, sizeof(int));
		printf("What parameter do you want to modify?\n1. Airplane Name\n2. Airplane No.\n3. Available Seats\n");
		printf("Your Choice: ");
		scanf("%d", &no_of_airplanes);
		write(sock, &no_of_airplanes, sizeof(int));
		if (no_of_airplanes == 2 || no_of_airplanes == 3)
		{
			read(sock, &no_of_airplanes, sizeof(int));
			printf("Current Value: %d\n", no_of_airplanes);
			printf("Enter Value: ");
			scanf("%d", &no_of_airplanes);
			write(sock, &no_of_airplanes, sizeof(int));
		}
		else
		{
			char name[20];
			read(sock, &name, sizeof(name));
			printf("Current Value: %s\n", name);
			printf("Enter Value: ");
			scanf("%s", name);
			write(sock, &name, sizeof(name));
		}
		read(sock, &opt, sizeof(opt));
		if (opt == 3)
			printf("Airplane Data Modified Successfully\n");
		while (getchar() != '\n')
			;
		getchar();
		return opt;
		break;
	}
	case 4:
	{
		write(sock, &opt, sizeof(opt));
		char pass[PASS_LENGTH], name[10];
		printf("Enetr the name: ");
		scanf("%s", name);
		strcpy(pass, getpass("Enter a password for the ADMIN account: "));
		write(sock, &name, sizeof(name));
		write(sock, &pass, sizeof(pass));
		read(sock, &opt, sizeof(opt));
		printf("The Account Number for this ADMIN is: %d\n", opt);
		read(sock, &opt, sizeof(opt));
		if (opt == 4)
			printf("Successfully created ADMIN account\n");
		while (getchar() != '\n')
			;
		getchar();
		return opt;
		break;
	}
	case 5:
	{
		int choice, users, id;
		write(sock, &opt, sizeof(opt));
		printf("What kind of account do you want to delete?\n");
		printf("1. Customer\n2. Agent\n3. Admin\n");
		printf("Your Choice: ");
		scanf("%d", &choice);
		write(sock, &choice, sizeof(choice));
		read(sock, &users, sizeof(users));
		while (users--)
		{
			char name[10];
			read(sock, &id, sizeof(id));
			read(sock, &name, sizeof(name));
			if (strcmp(name, "deleted") != 0)
				printf("%d\t%s\n", id, name);
		}
		printf("Enter the ID to delete: ");
		scanf("%d", &id);
		write(sock, &id, sizeof(id));
		read(sock, &opt, sizeof(opt));
		if (opt == 5)
			printf("Successfully deleted user\n");
		while (getchar() != '\n')
			;
		getchar();
		return opt;
	}
	case 6:
	{
		write(sock, &opt, sizeof(opt));
		read(sock, &opt, sizeof(opt));
		if (opt == 6)
			printf("Logged out successfully.\n");
		while (getchar() != '\n')
			;
		getchar();
		return -1;
		break;
	}
	default:
		return -1;
	}
}

int do_action(int sock, int opt)
{
	switch (opt)
	{
	case 1:
	{
		int airplanes, airplaneid, airplaneavseats, airplaneno, required_seats;
		char airplanename[20];
		char airplanedepart[50];
		char airplanearrive[50];
		int airplaneprice;
		char airplanedate[11];
		char airplane_boarding_time[7];
		write(sock, &opt, sizeof(opt));
		read(sock, &airplanes, sizeof(airplanes));
		printf("ID\tT_NO\tAV_SEAT\tAIRPLANE NAME\tDEPARTURE\tARRIVAL\t\tPRICE($)\tDATE\t\tBOARDING TIME\n");
		while (airplanes--)
		{
			read(sock, &airplaneid, sizeof(airplaneid));
			read(sock, &airplaneno, sizeof(airplaneno));
			read(sock, &airplaneavseats, sizeof(airplaneavseats));
			read(sock, &airplanename, sizeof(airplanename));
			read(sock, &airplanedepart, sizeof(airplanedepart));
			read(sock, &airplanearrive, sizeof(airplanearrive));
			read(sock, &airplaneprice, sizeof(airplaneprice));
			read(sock, &airplanedate, sizeof(airplanedate));
			read(sock, &airplane_boarding_time, sizeof(airplane_boarding_time));
			if (strcmp(airplanename, "deleted") != 0)
				printf("%-3d\t%-5d\t%-7d\t%-15s\t%-10s\t%-10s\t%-8d\t%-10s\t%-20s\n", airplaneid, airplaneno, airplaneavseats, airplanename, airplanedepart, airplanearrive, airplaneprice, airplanedate, airplane_boarding_time);
		}
		int option;
		printf("++++ OPTIONS ++++\n");
		printf("1. Search\n");
		printf("2. Book\n");
		printf("Your Choice: ");
		scanf("%d", &option);
		switch(option){
		case 1:
			printf("What information do you want to search ?");
			//oke
			break;
		case 2:
			printf("Enter you email: ");
			char mail[50];
			char command[200];
			scanf("%s", mail);
			printf("Enter the airplane ID: ");
			scanf("%d", &airplaneid);
			write(sock, &airplaneid, sizeof(airplaneid));
			read(sock, &airplaneavseats, sizeof(airplaneavseats));
			printf("Enter the number of seats: ");
			scanf("%d", &required_seats);
			if (airplaneavseats >= required_seats && required_seats > 0)
				write(sock, &required_seats, sizeof(required_seats));
			else
			{
				required_seats = -1;
				write(sock, &required_seats, sizeof(required_seats));
			}
			read(sock, &opt, sizeof(opt));
			sprintf(command,
            "curl -s --url 'smtp://smtp.gmail.com:587' --ssl-reqd "
            "--mail-from 'toniminh161200@gmail.com' --mail-rcpt %s "
            "--user 'toniminh161200@gmail.com:tkxj thcp fpzf exna' --tlsv1.2 --upload-file mail.html",
            mail);
			system(command);
			if(system(command) == 0){
				printf("Email has been successfully sent to: %s\n", mail);
			} else {
				printf("Failed to send email to: %s\n", mail);
			}
			if (opt == 1) {
				printf("Tickets booked in advance successfully\n");
				sprintf(command,
            "curl -s --url 'smtp://smtp.gmail.com:587' --ssl-reqd "
            "--mail-from 'toniminh161200@gmail.com' --mail-rcpt %s "
            "--user 'toniminh161200@gmail.com:tkxj thcp fpzf exna' --tlsv1.2 --upload-file mail.html",
            mail);
				system(command);
				if(system(command) == 0){
					printf("Email has been successfully sent to: %s\n", mail);
				} else {
					printf("Failed to send email to: %s\n", mail);
				}
				//system("curl -s --url 'smtp://smtp.gmail.com:587' --ssl-reqd --mail-from 'toniminh161200@gmail.com' --mail-rcpt '%s' --user 'toniminh161200@gmail.com:tkxj thcp fpzf exna' --tlsv1.2 --upload-file mail.html")
				printf("Please choose payment's method:\n");
				printf("1. Cash\n");
				printf("2. Payment card\n");
				printf("3. Onine banking\n");
				int option2;
				scanf("%d", &option2);
				switch(option2){
				case 1:
					printf("Please go to HUST to pay your bill within 2 days\n");
					break;
				case 2:	
					int card_number,CVV;
					printf("Please enter your card number:\n");
					scanf("%d", &card_number);
					printf("Please enter your CVV:\n");
					scanf("%d", &CVV);
					printf("Payment successfully\n");
					break;
				case 3:
					printf("Please transfer to this banking account: 99999999999 - VCB");
					break;
				}
			}
			else
				printf("Tickets were not booked. Please try again.\n");
			printf("Press any key to continue...\n");
			while (getchar() != '\n')
				;
			getchar()
			;
			while (!getchar())
				;
			return 1;
			break;
		}
	}
	case 2:
	{

		write(sock, &opt, sizeof(opt));
		view_booking(sock);
		read(sock, &opt, sizeof(opt));
		return 2;
	}
	case 3:
	{

		int val;
		write(sock, &opt, sizeof(opt));
		view_booking(sock);
		printf("Enter the booking id to be updated: ");
		scanf("%d", &val);
		write(sock, &val, sizeof(int));
		printf("What information do you want to update:\n1.Increase No of Seats\n2. Decrease No of Seats\nYour Choice: ");
		scanf("%d", &val);
		write(sock, &val, sizeof(int));
		if (val == 1)
		{
			printf("How many tickets do you want to increase: ");
			scanf("%d", &val);
			write(sock, &val, sizeof(int));
		}
		else if (val == 2)
		{
			printf("How many tickets do you want to decrease: ");
			scanf("%d", &val);
			write(sock, &val, sizeof(int));
		}
		read(sock, &opt, sizeof(opt));
		if (opt == -2)
			printf("Operation failed. No more available seats\n");
		else
			printf("Operation succeded.\n");
		while (getchar() != '\n')
			;
		getchar();
		return 3;
	}
	case 4:
	{
		write(sock, &opt, sizeof(opt));
		view_booking(sock);
		int val;
		printf("Enter the booking id to be deleted: ");
		scanf("%d", &val);
		write(sock, &val, sizeof(int));
		read(sock, &opt, sizeof(opt));
		if (opt == -2)
			printf("Operation failed. No more available seats\n");
		else
			printf("Operation succeded.\n");
		while (getchar() != '\n')
			;
		getchar();
		return 3;
	}
	case 5:
	{
		int airplanes, airplaneid, airplaneavseats, airplaneno, required_seats;
		char airplanename[20];
		char airplanedepart[50];
		char airplanearrive[50];
		int airplaneprice;
		char airplanedate[11];
		char airplane_boarding_time[7];
		write(sock, &opt, sizeof(opt));
		read(sock, &airplanes, sizeof(airplanes));
		printf("ID\tT_NO\tAV_SEAT\tAIRPLANE NAME\t\tDEPARTURE\t\tARRIVAL\tPRICE($)\tDATE\tBOARDING TIME\n");
		while (airplanes--)
		{
			read(sock, &airplaneid, sizeof(airplaneid));
			read(sock, &airplaneno, sizeof(airplaneno));
			read(sock, &airplaneavseats, sizeof(airplaneavseats));
			read(sock, &airplanename, sizeof(airplanename));
			read(sock, &airplanedepart, sizeof(airplanedepart));
			read(sock, &airplanearrive, sizeof(airplanearrive));
			read(sock, &airplaneprice, sizeof(airplaneprice));
			read(sock, &airplanedate, sizeof(airplanedate));
			read(sock, &airplane_boarding_time, sizeof(airplane_boarding_time));
			if (strcmp(airplanename, "deleted") != 0)
				printf("%d\t%d\t%d\t%s\t\t%s\t\t%s\t%d\t%s\t%s\n", airplaneid, airplaneno, airplaneavseats, airplanename, airplanedepart, airplanearrive, airplaneprice, airplanedate, airplane_boarding_time);
		}

		int sub_options = -1;

		printf("\n\t\t\t###Searching menu###\n");
		printf("1. Search by airplane name.\n");
		printf("2. Search by depature.\n");
		printf("3. Search by arrival\n");
		printf("4. Search by date.\n");
		printf("0. Exit searching.\n");
		printf("Enter options of searching: ");
		scanf("%d", &sub_options);
		if (sub_options != 0)
		{
			menu_search(sock, sub_options);
			printf("Search successful\n");
		}

		while (getchar() != '\n')
			;
		getchar();
		return 6;
	}
	case 6:
	{
		write(sock, &opt, sizeof(opt));
		read(sock, &opt, sizeof(opt));
		if (opt == 5)
			printf("Logged out successfully.\n");
		while (getchar() != '\n')
			;
		getchar();
		return -1;
		break;
	}
	default:
		return -1;
	}
}

int menu_search(int sock, int search_option)
{
	char search_query[50];

	printf("Enter search query: ");
	scanf("%s", search_query);
	write(sock, &search_option, sizeof(search_option));
	write(sock, &search_query, sizeof(search_query));
	printf("Search query: %s\n", search_query);
	int found_airplanes;
	read(sock, &found_airplanes, sizeof(found_airplanes));

	if (found_airplanes == 0)
	{
		printf("No matching airplanes found.\n");
		return 0;
	}
	int airplanes, airplaneid, airplaneavseats, airplaneno, required_seats;
	char airplanename[20];
	char airplanedepart[50];
	char airplanearrive[50];
	int airplaneprice;
	char airplanedate[11];
	char airplane_boarding_time[7];
	printf("ID\tT_NO\tAV_SEAT\tAIRPLANE NAME\t\tDEPARTURE\t\tARRIVAL\tPRICE($)\tDATE\tBOARDING TIME\n");

	for (int i = 0; i < found_airplanes; i++)
	{
		read(sock, &airplaneid, sizeof(airplaneid));
		read(sock, &airplaneno, sizeof(airplaneno));
		read(sock, &airplaneavseats, sizeof(airplaneavseats));
		read(sock, &airplanename, sizeof(airplanename));
		read(sock, &airplanedepart, sizeof(airplanedepart));
		read(sock, &airplanearrive, sizeof(airplanearrive));
		read(sock, &airplaneprice, sizeof(airplaneprice));
		read(sock, &airplanedate, sizeof(airplanedate));
		read(sock, &airplane_boarding_time, sizeof(airplane_boarding_time));

		if (strcmp(airplanename, "deleted") != 0)
			printf("%d\t%d\t%d\t%s\t\t%s\t\t%s\t%d\t%s\t%s\n", airplaneid, airplaneno, airplaneavseats, airplanename, airplanedepart, airplanearrive, airplaneprice, airplanedate, airplane_boarding_time);
	}
}

void view_booking(int sock)
{
	int entries;
	read(sock, &entries, sizeof(int));
	if (entries == 0)
		printf("No records found.\n");
	else
		printf("Your recent %d bookings are :\n", entries);
	while (!getchar())
		;
	while (entries--)
	{
		int bid, bks_seat, bke_seat, cancelled;
		char airplanename[20];
		char airplanedepart[50];
		char airplanearrive[50];
		int airplaneprice;
		char airplanedate[11];
		char airplane_boarding_time[7];
		read(sock, &bid, sizeof(bid));
		read(sock, &airplanename, sizeof(airplanename));
		read(sock, &bks_seat, sizeof(int));
		read(sock, &bke_seat, sizeof(int));
		read(sock, &cancelled, sizeof(int));
		read(sock, &airplanedepart, sizeof(airplanedepart));
		read(sock, &airplanearrive, sizeof(airplanearrive));
		read(sock, &airplaneprice, sizeof(int));
		read(sock, &airplanedate, sizeof(airplanedate));
		read(sock, &airplane_boarding_time, sizeof(airplane_boarding_time));
		if (!cancelled)
			printf("BookingID: %d\t1st Ticket: %d\tLast Ticket: %d\tAIRPLANE :%s\tDEPARTURE :%s\tARRIVAL :%s\tPRICE :%d\tDATE: %s\tBOARDING TIME: %s\n", bid, bks_seat, bke_seat, airplanename, airplanedepart, airplanearrive, airplaneprice, airplanedate, airplane_boarding_time);
	}
	printf("Press any key to continue...\n");
	while (getchar() != '\n')
		;
	getchar();
}
