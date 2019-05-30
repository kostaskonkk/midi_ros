//*****************************************//
//  midi_input.cpp
//  by Kostas Konstantinidis, 2019.
//
//  Simple ROS node to check MIDI input and publish it under the /midi topic.
//
//*****************************************//

#include "ros/ros.h"
#include "RtMidi.h"
#include <cstdlib>  
#include <iostream>
#include <std_msgs/Int8.h>
#include <midi_ros/midi.h>

midi_ros::midi midi_msg;


void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: cmidiin <port>\n";
  std::cout << "    where port = the device to use (default = 0).\n\n";
  exit( 0 );
}

void mycallback( double deltatime, std::vector< unsigned char > *message, void *userData)
{
  unsigned int nBytes = message->size();
  std_msgs::Int8 msg;

  for ( unsigned int i=0; i<nBytes; i++ ){

   //  std::cout << "Byte " << i << " = " << (int)messageat(i) << ", ";
	  // msg.data = (int)message->at(i);

    if(i =1) midi_msg.knob = (int)message->at(i);
    if(i =2) midi_msg.value = (int)message->at(i);

    if((int)message->at(1) == 45 && (int)message->at(2) == 127) system("roslaunch datmo 5_exp.launch");
    if((int)message->at(1) == 46 && (int)message->at(2) == 127) system("rosrun datmo odom_node");

    // if((int)message->at(1) == 44 && (int)message->at(2) == 127) system("pkill roslaunch");
  
  }


  ros::Publisher *callback_midi_pub; 
  callback_midi_pub = reinterpret_cast<ros::Publisher *>(userData);

  // ros::Publisher callback_midi_pub = *midipub;

  callback_midi_pub->publish(midi_msg);


  ros::spinOnce();
 
  
}

// This function should be embedded in a try/catch block in case of
// an exception.  It offers the user a choice of MIDI ports to open.
// It returns false if there are no ports available.
bool chooseMidiPort( RtMidiIn *rtmidi );

int main(int argc, char **argv)
{	

  RtMidiIn *midiin = 0;

  // Minimal command-line check.
  if ( argc > 2 ) usage();

  try {

    // RtMidiIn constructor
    midiin = new RtMidiIn();

    // Call function to select port.
    // if ( chooseMidiPort( midiin ) == false ) goto cleanup;
    midiin->openPort( 1 );


  	// Initiate ROS
  	ros::init(argc, argv, "midi_listener");
  	ros::NodeHandle nh; 
    ros::Publisher midi_pub;

    midi_pub = nh.advertise<midi_ros::midi>("midi", 10);

    ros::Publisher *userData;


    userData = & midi_pub;



    midiin->setCallback( &mycallback, userData );

    //Ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( true, true, true );

    std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
    char input;
    std::cin.get(input);


    // bool bvalue;
    // if(byte1 == 67 && byte2> 100){
    //   if (nh.getParam("/FB",bvalue)) {
    //     nh.setParam("/FB", !bvalue);
    //     std::cout << "\nFB = "<< !bvalue;


    //   }
    //   else nh.setParam("/FB", true);
    // }
    

    // double value;
    // double t_la = 0.6;
    // if(byte1 == 3){
    //   if (nh.getParam("/t_la",value)) {
    //     double byte = byte2;
    //     t_la = 0.6 + (byte-64)/200;
    //     std::cout << "\nt_la = "<< t_la;
    //     nh.setParam("/t_la", t_la);
    //   }
    //   else nh.setParam("/t_la", t_la);
    // }

    // double k_p1 = 1.3;
    // if(byte1 == 3){
    //   if (nh.getParam("/k_p1",value)) {
    //     double byte = byte2;
    //     k_p1 = 1 + (byte-64)/200;
    //     std::cout << "\nk_p1 = "<< k_p1;
    //     nh.setParam("/k_p1", k_p1);
    //   }
    //   else nh.setParam("/k_p1", k_p1);
    // }
    

  }
  catch ( RtMidiError &error ) {
    error.printMessage();
  }
	
 cleanup:

 delete midiin;

  return 0;
}


bool chooseMidiPort( RtMidiIn *rtmidi )
{
  std::cout << "\nWould you like to open a virtual input port? [y/N] ";

  std::string keyHit;
  std::getline( std::cin, keyHit );
  if ( keyHit == "y" ) {
    rtmidi->openVirtualPort();
    return true;
  }

  std::string portName;
  unsigned int i = 0, nPorts = rtmidi->getPortCount();
  if ( nPorts == 0 ) {
    std::cout << "No input ports available!" << std::endl;
    return false;
  }

  if ( nPorts == 1 ) {
    std::cout << "\nOpening " << rtmidi->getPortName() << std::endl;
  }
  else {
    for ( i=0; i<nPorts; i++ ) {
      portName = rtmidi->getPortName(i);
      std::cout << "  Input port #" << i << ": " << portName << '\n';
    }

    do {
      std::cout << "\nChoose a port number: ";
      std::cin >> i;
    } while ( i >= nPorts );
    std::getline( std::cin, keyHit );  // used to clear out stdin
  }

  rtmidi->openPort( i );

  return true;
}
