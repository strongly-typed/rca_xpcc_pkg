#include "ros/ros.h"
#include "std_msgs/String.h"
#include "rca_xpcc_pkg/Can.h"

// XPCC
#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/hosted/canusb.hpp>

xpcc::hosted::CanUsb canUsb;

void
can2usbCallback(const rca_xpcc_pkg::Can::ConstPtr& msg)
{
	ROS_INFO("I got %d with %d bytes", msg->identifier, msg->data.size());
	
	// Build xpcc CAN message
	xpcc::can::Message message;
	message.setIdentifier(msg->identifier);
	message.setRemoteTransmitRequest(msg->rtr);
	message.setExtended(msg->extended);
	
	for (std::size_t ii = 0; ii < msg->data.size(); ++ii) {
		message.data[ii] = msg->data[ii];
	}
	message.setLength(msg->data.size());
	
	ROS_DEBUG("XPCC Can message is:");
	ROS_DEBUG("  Identifier = %d", message.getIdentifier());
	ROS_DEBUG("  RTR        = %d", message.isRemoteTransmitRequest());
	ROS_DEBUG("  extended   = %d", message.isExtended());
	ROS_DEBUG("  Length     = %d", message.getLength());
	ROS_DEBUG("  DATA       = %02x %02x %02x %02x %02x %02x %02x %02x",
		message.data[0], message.data[1], message.data[2], message.data[3],
		message.data[4], message.data[5], message.data[6], message.data[7] );
	
	// Send
	if (canUsb.isReadyToSend())
	{
		if (canUsb.sendMessage(message)) {
			ROS_INFO("Send ok");
		} else {
			ROS_INFO("Send fail");
		}
	}
	else
	{
		ROS_ERROR("Could not send CAN message because canUsb is not ready to send.");
	}
}

/**
 * Simple ROS node that uses xpcc CAN2USB parser.
 * 
 * It can receive CAN messages on the can2usb/tx topic and sends them to the CAN bus.
 * Messages on the CAN bus (from any other node) is received and broadcasted on the can2usb/rx topic.
 *
 */
int
main(int argc, char **argv)
{
    ros::init(argc, argv, "can2usb");
    ros::NodeHandle n("~");
    ros::Publisher  can2usb_pub = n.advertise<rca_xpcc_pkg::Can>("rx", 1000);
    ros::Subscriber can2usb_sub = n.subscribe("tx", 1000, can2usbCallback);
    ros::Rate loop_rate(10);

    ROS_INFO("Hello CAN2USB");
    
    std::string device_string;
    n.param("device_string", device_string, std::string("/dev/ttyUSB0"));
    
    int canBusBaudRate;
    n.param("can_bus_baudrate", canBusBaudRate, 125000);
    
	if (!canUsb.open(device_string, canBusBaudRate)) {
    	ROS_ERROR_STREAM("Could not open port " << device_string);
    }

    while (ros::ok())
    {
        // Process messages from CAN bus
		while (canUsb.isMessageAvailable())
		{
			xpcc::can::Message message;
			canUsb.getMessage(message);
			
            rca_xpcc_pkg::Can msg;
            msg.header.stamp = ros::Time::now();
            msg.identifier   = message.getIdentifier();
            msg.rtr          = message.isRemoteTransmitRequest();
            msg.extended     = message.isExtended();
            
            for (std::size_t ii = 0; ii < message.getLength(); ++ii) {
                msg.data.push_back(message.data[ii]);
            }
            
            can2usb_pub.publish(msg);
		}

        ros::spinOnce();

        loop_rate.sleep();
    }
    
    return 0;
}

