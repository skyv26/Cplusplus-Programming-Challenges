#include <fcntl.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <termios.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>

using namespace std;

class Uart{
    private:
        string portname;
        size_t baud_rate;
        int    file_descriptor;
        int    byte_counter;

    public:
        Uart(string portname, size_t baud_rate){
            this->portname=portname;
            this->baud_rate=baud_rate;
            this->file_descriptor=open(this->portname.c_str(), O_RDWR);
            
            if(this->file_descriptor<0){
                cout<<"Port is Busy or Can't open for communication !!";
                exit(0);
            } else {
                cout<<"Connection Succeed!!"<<endl;
            }

            struct termios options;

            tcgetattr(this->file_descriptor, &options);
            options.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
            options.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
            options.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
            options.c_cflag |= CS8; // 8 bits per byte (most common)
            options.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
            options.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

            options.c_lflag &= ~ICANON;
            options.c_lflag &= ~ECHO; // Disable echo
            options.c_lflag &= ~ECHOE; // Disable erasure
            options.c_lflag &= ~ECHONL; // Disable new-line echo
            options.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
            options.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
            options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

            options.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
            options.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
            // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
            // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

            options.c_cc[VTIME] = 5;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
            options.c_cc[VMIN] = 0;

            // Set in/out baud rate to be 9600
            cfsetispeed(&options, this->baud_rate);
            cfsetospeed(&options, this->baud_rate);
            // options.c_cflag = this->baud_rate | CS8 | CREAD | CLOCAL;
            // options.c_cflag &= ~CRTSCTS;
            // options.c_iflag = IGNPAR | ICRNL;
            // options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
            // options.c_oflag &= ~OPOST;
            // options.c_oflag &= ~ONLCR;

            tcsetattr(this->file_descriptor, TCSANOW, &options);

        }
        ~Uart(){
            close(this->file_descriptor);
            cout<<"Connection Closed !!"<<endl;
        }

        string readline(){
            char recieve[500];
            char* data=(char *)"\r";
            memset(&recieve, '\r', sizeof(recieve));
            int array_size=sizeof(recieve) / sizeof(char);
            this->byte_counter=read(this->file_descriptor, &recieve, sizeof(recieve));
            if(this->byte_counter>0){
                data = (char *) malloc(this->byte_counter*sizeof(char));
                data=recieve;
                return data;
            }
            string temp(data);
            return temp;
      }

      void writeData(string text){
          int count=0;
	  const char * ch=text.c_str();
	  count=write(this->file_descriptor, ch, strlen(ch));
          if(count<0){
              cout<<"Error Occured !!";
              exit(0);
          }
      }
      void endSMS(int charr){
	  int count=0;
	  const char * ch=to_string(charr).c_str();
	  count=write(this->file_descriptor, ch, strlen(ch));
	  if(count<0){
	      cout<<"Error Occured !!";
	      exit(0);
          }
      }
};


int main(int argc, char **argv){
    Uart gsm("/dev/ttyUSB0",B115200);
    int option;
    string ph,data,msg;
    while(1){
      cout<<"Please select an option \n";
      cout<<"1. Call"<<endl;
      cout<<"2. SMS"<<endl;
      cout<<"Enter choice : ";
      cin>>option;
      
      switch(option){
	  case 1: {cout<<"Enter phone number : ";
		  cin>>ph;
		  data="ATD"+ph+";\r";
		  cout<<data<<endl;
		  gsm.writeData(data);
		  for(int i=0; i<5; i++){
		    cout<<gsm.readline()<<endl;
		  system("sleep 1");
		  }
	  break;}
	  case 2: {
	          cout<<"Enter phone number : ";
		  cin>>ph;
		  cin.ignore();
		  cout<<"Type your message : ";
		  getline(cin, msg);
		  gsm.writeData("AT+CMGF=1\r"); // Configuring TEXT mode
		  cout<<"Text Mode Enabled"<<flush<<endl;
		  sleep(3);
		  cout<<gsm.readline()<<flush<<endl;
		  data="AT+CMGS=\""+ph+"\""+'\r';
		  gsm.writeData(data);//change ZZ with country code and xxxxxxxxxxx with phone number to sm
		  sleep(3);
		  cout<<gsm.readline()<<endl;
		  gsm.writeData(msg+(char)26);
		  sleep(3);
		  cout<<gsm.readline()<<flush<<endl;
		  for(int i=0; i<5; i++){
		  cout<<gsm.readline()<<flush<<endl;
		  sleep(2);
	 	 }
		  cout<<"message sent…";
		  
	  break;
	  }
	  default: cout<<"Invalid Option!"<<endl;
	  
      }
    }
    return 0;
}

